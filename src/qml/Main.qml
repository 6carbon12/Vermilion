import QtQuick
import QtQuick.Controls
import QtMultimedia
import Vermilion

ApplicationWindow {
	width: 640
	height: 480
	visible: true

	MediaDevices {
		id: devices
	}

	MediaPlayer {
		id: audioPlayer
		audioOutput: AudioOutput {
			device: devices.defaultAudioOutput
		}

		onErrorOccurred: {
			console.log("Playback error: ", error, errorString);
		}
	}

	Column {
		anchors.centerIn: parent
		spacing: 10

		TextField {
			id: urlInput
			placeholderText: "Enter video URL... if you want"
			width: 300
		}

		Button {
			text: "Play Audio"
			onClicked: {
				if (urlInput.text !== "") {
					YtDLP.requestExtraction(urlInput.text);
				}
			}
		}

		Button {
			text: audioPlayer.playbackState === MediaPlayer.PlayingState ? "Pause" : "Play"
			enabled: audioPlayer.source.toString() !== ""
			onClicked: {
				if (audioPlayer.playbackState === MediaPlayer.PlayingState) {
					audioPlayer.pause();
				} else {
					audioPlayer.play();
				}
			}
		}
	}

	Connections {
		target: YtDLP

		function onExtractionSuccess(url) {
			console.log("Extraction succeeded, starting playback...");
			audioPlayer.source = url;
			audioPlayer.play();
		}

		function onExtractionFailed(error) {
			console.error("Extraction failed:", error);
		}
	}
}
