#include "PlaylistEngine.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

Q_LOGGING_CATEGORY(PlaylistEngine_l, "PlaylistEngine")

PlaylistEngine::PlaylistEngine(QObject *parent) : QObject(parent) {
  QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/playlists.db";

  m_db = QSqlDatabase::addDatabase("QSQLITE");
  m_db.setDatabaseName(dbPath);

  if (!m_db.open()) {
    m_lastError = "DB connection failed.\nDB: " + m_db.lastError().text();
    return;
  }

  QSqlQuery query(m_db);
  if (!query.exec("PRAGMA foreign_keys = ON;")) {
    m_lastError = "Failed to enable foreign keys.\nDB: " + m_db.lastError().text();
    return;
  }

  for (const QString &sql : s_schemaStatements) {
    if (!query.exec(sql)) {
      m_lastError = "Failed to execute basic schema statements.\nDB:" + query.lastError().text();
      return;
    }
  }
}

PlaylistEngine::~PlaylistEngine() { m_db.close(); }

bool PlaylistEngine::createPlaylist(const Playlist &playlist) {
  if (playlist.id.isEmpty() || playlist.name.trimmed().isEmpty()) {
    m_lastError = "Invalid playlist metadata provided.\nDB: " + m_db.lastError().text();
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB: " + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
        INSERT OR IGNORE INTO playlists (id, name, cover_uri, type)
        VALUES (:id, :name, :cover_uri, :type);
    )");

  QString typeStr = Core::playlistTypeToString(playlist.type);

  query.bindValue(":id", playlist.id);
  query.bindValue(":name", playlist.name.trimmed());
  query.bindValue(":cover_uri", playlist.artUrl);
  query.bindValue(":type", typeStr);

  if (!query.exec()) {
    m_lastError = "Failed to create playlist.\nDB: " + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction.\nDB: " + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT playlistUpdated(playlist.id);
  return true;
}

bool PlaylistEngine::addToPlaylist(const Core::Track &track, const QString &playlistId) {
  if (track.id.isEmpty() || playlistId.isEmpty()) {
    m_lastError = "addToPlaylist: Track ID or Playlist ID cannot be empty.";
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB" + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);

  QJsonArray artistsArray;
  for (int i = 0; i < track.artistsNum && i < 8; ++i) {
    QJsonObject obj;
    obj["id"] = track.artists[i].id;
    obj["name"] = track.artists[i].name;
    artistsArray.append(obj);
  }

  QString jsonString = QString::fromUtf8(QJsonDocument(artistsArray).toJson(QJsonDocument::Compact));

  query.prepare(R"(
    INSERT INTO tracks (
      id, is_explicit, url, title, views,
      cover_art_url, duration_string, duration, artists_json
    ) VALUES (
      :id, :is_explicit, :url, :title, :views,
      :cover_art_url, :duration_string, :duration, :artists_json
    )
    ON CONFLICT(id) DO UPDATE SET
      url = excluded.url,
      title = excluded.title,
      views = excluded.views,
      cover_art_url = excluded.cover_art_url,
      duration_string = excluded.duration_string,
      duration = excluded.duration,
      artists_json = excluded.artists_json;
    )");

  query.bindValue(":id", track.id);
  query.bindValue(":is_explicit", track.isExplict);
  query.bindValue(":url", track.url);
  query.bindValue(":title", track.title);
  query.bindValue(":views", track.views);
  query.bindValue(":cover_art_url", track.coverArtUrl);
  query.bindValue(":duration_string", track.durationString);
  query.bindValue(":duration", static_cast<qint64>(track.duration));
  query.bindValue(":artists_json", jsonString);

  if (!query.exec()) {
    m_lastError = "Failed to insert into tracks table.\nDB:" + query.lastError().text();
    m_db.rollback();
    return false;
  }

  query.prepare(R"(
      INSERT OR IGNORE INTO playlist_tracks (playlist_id, track_id, position)
      VALUES (
        :playlist_id,
        :track_id,
        (SELECT COALESCE(MAX(position), 0) + 1 FROM playlist_tracks WHERE playlist_id = :playlist_id)
      );
    )");
  query.bindValue(":playlist_id", playlistId);
  query.bindValue(":track_id", track.id);

  if (!query.exec()) {
    m_lastError = "Failed to add track to playlist.\nDB:" + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction.\nDB:" + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT tracksUpdated(playlistId);
  return true;
}

bool PlaylistEngine::removeFromPlaylist(const QString &trackId, const QString &playlistId) {
  if (trackId.isEmpty() || playlistId.isEmpty()) {
    m_lastError = "removeFromPlaylis: Track ID or Playlist ID cannot be empty.";
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB: " + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
      DELETE FROM playlist_tracks
      WHERE playlist_id = :playlist_id AND track_id = :track_id;
    )");

  query.bindValue(":playlist_id", playlistId);
  query.bindValue(":track_id", trackId);

  if (!query.exec()) {
    m_lastError = "Failed to remove track from playlist.\nDB: " + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction: " + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT tracksUpdated(playlistId);
  return true;
}

bool PlaylistEngine::renamePlaylist(const QString &playlistId, const QString &newName) {
  if (playlistId.isEmpty() || newName.trimmed().isEmpty()) {
    m_lastError = "Invalid arguments for renamePlaylist.\nDB: " + m_db.lastError().text();
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB: " + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
      UPDATE playlists
      SET name = :newName
      WHERE id = :playlistId;
    )");

  query.bindValue(":newName", newName.trimmed());
  query.bindValue(":playlistId", playlistId);

  if (!query.exec()) {
    m_lastError = "Failed to rename playlist.\nDB: " + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction.\nDB: " + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT playlistUpdated(playlistId);
  return true;
}

bool PlaylistEngine::updatePlaylistArt(const QString &playlistId, const QString &artPath) {
  if (playlistId.isEmpty()) {
    m_lastError = "Playlist ID cannot be empty.\nDB: " + m_db.lastError().text();
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB: " + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
      UPDATE playlists
      SET cover_uri = :artPath
      WHERE id = :playlistId;
    )");

  query.bindValue(":artPath", artPath);
  query.bindValue(":playlistId", playlistId);

  if (!query.exec()) {
    m_lastError = "Failed to update playlist cover art.\nDB: " + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction.\nDB: " + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT playlistUpdated(playlistId);
  return true;
}

bool PlaylistEngine::deletePlaylist(const QString &playlistId) {
  if (playlistId.isEmpty()) {
    m_lastError = "Playlist ID cannot be empty.\nDB: " + m_db.lastError().text();
    return false;
  }

  if (playlistId == QStringLiteral("liked")) {
    m_lastError = "Cannot delete default system playlist.\nDB: " + m_db.lastError().text();
    return false;
  }

  if (!m_db.transaction()) {
    m_lastError = "Failed to start transaction.\nDB: " + m_db.lastError().text();
    return false;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
      DELETE FROM playlists
      WHERE id = :playlistId;
    )");

  query.bindValue(":playlistId", playlistId);

  if (!query.exec()) {
    m_lastError = "Failed to delete playlist.\nDB: " + query.lastError().text();
    m_db.rollback();
    return false;
  }

  if (!m_db.commit()) {
    m_lastError = "Failed to commit transaction.\nDB: " + m_db.lastError().text();
    m_db.rollback();
    return false;
  }

  Q_EMIT playlistUpdated(playlistId);
  return true;
}

QList<Core::Playlist> PlaylistEngine::getPlaylists() const {
  QList<Core::Playlist> playlists;

  QSqlQuery query(m_db);
  query.prepare(R"(
      SELECT id, name, cover_uri, type, created_at
      FROM playlists
      ORDER BY created_at ASC;
    )");

  if (!query.exec()) {
    qCCritical(PlaylistEngine_l) << "Failed to fetch playlists.\nDB:" << query.lastError().text();
    return playlists;
  }

  while (query.next()) {
    Core::Playlist playlist;
    playlist.id = query.value("id").toString();
    playlist.name = query.value("name").toString();
    playlist.artUrl = query.value("cover_uri").toString();

    QString typeStr = query.value("type").toString();
    playlist.type = Core::playlistTypeFromString(typeStr);
    playlist.createdAt = query.value("created_at").toDateTime();

    playlists.append(playlist);
  }

  return playlists;
}

Core::Playlist PlaylistEngine::getPlaylistFromId(const QString &playlistId) const {
  Core::Playlist playlist;
  QSqlQuery query(m_db);
  query.prepare(R"(
      SELECT id, name, cover_uri, type, created_at
      FROM playlists
      WHERE id = :playlistId
    )");

  query.bindValue(":playlistId", playlistId);

  if (!query.exec()) {
    qCCritical(PlaylistEngine_l) << "Failed to fetch tracks for playlist:" << playlistId
                                 << "\nDB:" << query.lastError().text();
    return playlist;
  }

  query.next();

  playlist.id = query.value("id").toString();
  playlist.name = query.value("name").toString();
  playlist.artUrl = query.value("cover_uri").toString();
  QString typeStr = query.value("type").toString();
  playlist.type = Core::playlistTypeFromString(typeStr);
  playlist.createdAt = query.value("created_at").toDateTime();

  return playlist;
}

QList<Core::Track> PlaylistEngine::getTracksFromPlaylist(const QString &playlistId) const {
  QList<Core::Track> tracks;

  if (playlistId.isEmpty()) {
    return tracks;
  }

  QSqlQuery query(m_db);
  query.prepare(R"(
      SELECT t.id, t.is_explicit, t.url, t.title, t.views,
             t.cover_art_url, t.duration_string, t.duration, t.artists_json
      FROM tracks t
      INNER JOIN playlist_tracks pt ON t.id = pt.track_id
      WHERE pt.playlist_id = :playlistId
      ORDER BY pt.position ASC;
    )");

  query.bindValue(":playlistId", playlistId);

  if (!query.exec()) {
    qCCritical(PlaylistEngine_l) << "Failed to fetch tracks for playlist:" << playlistId
                                 << "\nDB:" << query.lastError().text();
    return tracks;
  }

  while (query.next()) {
    Core::Track track;
    track.id = query.value("id").toString();
    track.isExplict = query.value("is_explicit").toBool();
    track.url = query.value("url").toString();
    track.title = query.value("title").toString();
    track.views = query.value("views").toString();
    track.coverArtUrl = query.value("cover_art_url").toString();
    track.durationString = query.value("duration_string").toString();
    track.duration = query.value("duration").toLongLong();

    QString jsonStr = query.value("artists_json").toString();
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (doc.isArray()) {
      QJsonArray artistsArray = doc.array();
      int count = 0;
      for (const QJsonValue &val : artistsArray) {
        if (count >= 8)
          break;
        QJsonObject obj = val.toObject();
        track.artists[count].id = obj["id"].toString();
        track.artists[count].name = obj["name"].toString();
        count++;
      }
      track.artistsNum = count;
    } else {
      track.artistsNum = 0;
    }

    tracks.append(track);
  }

  return tracks;
}

QString PlaylistEngine::generateLocalPlaylistId() {
  QByteArray bytes;
  for (int i = 0; i < 24; i++) {
    bytes.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
  }
  // VL = Vermilion Local
  return "VL" + bytes.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}

QString PlaylistEngine::getLastError() const { return m_lastError; }
