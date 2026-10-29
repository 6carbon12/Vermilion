def get_watch_playlist_normalized(yt_instance, video_id):
    # Fetch most of the contents.
    watch_playlist = yt_instance.get_watch_playlist(
        videoId=video_id, radio=True)
    tracks = watch_playlist.get("tracks", [])

    # Get `isExplicit`.
    body = {"videoId": video_id,
            "enablePersistentPlaylistPanel": True, "isRtl": False}
    if watch_playlist.get("playlistId"):
        body["playlistId"] = watch_playlist["playlistId"]

    response = yt_instance._send_request("next", body)

    # Extract the data from YouTube response.
    explicit_map = {}
    try:
        results = response.get("contents", {}).get(
            "twoColumnWatchNextResults", {}).get("playlist", {}).get("playlist", {})
        contents = results.get("contents", [])
        for item in contents:
            renderer = item.get("playlistPanelVideoRenderer", {})
            v_id = renderer.get("videoId")
            if v_id:
                badges = renderer.get("badges", [])
                explicit_map[v_id] = any(
                    b.get("musicInlineBadgeRenderer", {}).get(
                        "icon", {}).get("iconType") == "MUSIC_EXPLICIT_BADGE"
                    for b in badges
                )
    except Exception:
        pass

    # Append the data into the tracks returned by `ytmusicapi`.
    # Also normalize some member names.
    for track in tracks:
        v_id = track.get("videoId")

        track["isExplicit"] = explicit_map.get(v_id, False)

        if "thumbnail" in track and "thumbnails" not in track:
            track["thumbnails"] = track["thumbnail"]

        length_str = track.get("length", "")
        track["duration"] = length_str

        seconds = 0
        if length_str:
            parts = length_str.split(":")
            try:
                if len(parts) == 2:
                    seconds = int(parts[0]) * 60 + int(parts[1])
                elif len(parts) == 3:
                    seconds = int(parts[0]) * 3600 + \
                        int(parts[1]) * 60 + int(parts[2])
            except ValueError:
                seconds = 0
        track["duration_seconds"] = seconds
        # Make it empty instead of None so `getStrMember` doesn't complain.
        track["views"] = ""

    return watch_playlist
