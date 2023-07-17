

var videoElement1 = document.querySelector('video#video1');
var videoElement2 = document.querySelector('video#video2');
var videoSelect1 = document.querySelector('select#video1');
var videoSelect2 = document.querySelector('select#video2');
const canvasElement = document.getElementById('canvas3d');
const canvas1 = document.getElementById('canvas1');
const canvas2 = document.getElementById('canvas2');
const context3d = canvasElement.getContext('2d');
const context1 = canvas1.getContext('2d');
const context2 = canvas2.getContext('2d');

videoSelect1.onchange = getStream;
videoSelect2.onchange = getStream;

getStream().then(getDevices).then(gotDevices);

function getDevices() {
  return navigator.mediaDevices.enumerateDevices();
}

function gotDevices(deviceInfos) {
  window.deviceInfos = deviceInfos; // make available to console
  console.log('Available input and output devices:', deviceInfos);
  for (const deviceInfo of deviceInfos) {
    const option1 = document.createElement('option');
	const option2 = document.createElement('option');
    option1.value1 = deviceInfo.deviceId;
	option2.value1 = deviceInfo.deviceId;
    if (deviceInfo.kind === 'videoinput') {
      option1.text = deviceInfo.label || `Camera ${videoSelect.length + 1}`;
	  option2.text = deviceInfo.label || `Camera ${videoSelect.length + 1}`;
      videoSelect1.appendChild(option1);
	  videoSelect2.appendChild(option2);
    }
  }
}

function getStream() {
  if (window.stream) {
    window.stream.getTracks().forEach(track => {
      track.stop();
    });
  }
  const videoSource1 = videoSelect1.value;
  const constraints = {
    audio: {deviceId: undefined},
    video: { facingMode: 'environment' }//video: {deviceId: videoSource1 ? {exact: videoSource1} : undefined}
  };
  return navigator.mediaDevices.getUserMedia(constraints).
    then(gotStream1).catch(handleError);
}

function getStream2() {

  const videoSource2 = videoSelect2.value;
  const constraints2 = {
    audio: {deviceId: undefined},
    video: { facingMode: 'user' }//video: {deviceId: videoSource2 ? {exact: videoSource2} : undefined}
  };
  navigator.mediaDevices.getUserMedia(constraints2).
    then(gotStream2).catch(handleError);
}

function gotStream1(stream) {
  window.stream = stream;
  videoSelect1.selectedIndex = [...videoSelect1.options].findIndex(option => option.text === stream.getVideoTracks()[0].label);
  videoElement1.srcObject = stream;
  
  getStream2();
}

function gotStream2(stream) {
  window.stream = stream;
  videoSelect2.selectedIndex = [...videoSelect2.options].findIndex(option => option.text === stream.getVideoTracks()[0].label);
  videoElement2.srcObject = stream;
}

function handleError(error) {
  console.error('Error: ', error);
}


videoElement1.addEventListener('loadedmetadata', function() {
  canvas1.width = videoElement1.videoWidth;
  canvas1.height = videoElement1.videoHeight;
  canvasElement.width = videoElement1.videoWidth;
  canvasElement.height = videoElement1.videoHeight;

  cloneVideoFrame();
});

videoElement2.addEventListener('loadedmetadata', function() {
  canvas2.width = videoElement1.videoWidth;
  canvas2.height = videoElement1.videoHeight;
  canvasElement.width = videoElement1.videoWidth;
  canvasElement.height = videoElement1.videoHeight;

  cloneVideoFrame();
});

var totalFlow = 0;

function cloneVideoFrame() {
	totalFlow++;
	
	if (totalFlow<2) return;
	
	context1.clearRect(0, 0, canvas1.width, canvas1.height);
	context1.drawImage(videoElement1, 0, 0, canvas1.width, canvas1.height);
	const imageData = context1.getImageData(0, 0, canvas1.width, canvas1.height);
	const dataRed = imageData.data;

	const imageDataCombined = context3d.createImageData(canvasElement.width, canvasElement.height);
	const dataCombined = imageDataCombined.data;

	for (let i = 0; i < dataRed.length; i += 4) {
		if (document.getElementById("colType").checked == 1)	// orange/blue
		{
			dataCombined[i] = dataRed[i]; // Red channel
			dataCombined[i + 1] = dataRed[i+1]; // Green channel set to 0
			dataCombined[i + 2] = 0; // Blue channel set to 0
		}
		else
		{
			dataCombined[i] = dataRed[i]; // Red channel
			dataCombined[i + 1] = 0; // Green channel set to 0
			dataCombined[i + 2] = 0; // Blue channel set to 0
		}
	  dataCombined[i + 3] = 255; // Alpha channel
	}
	//contextCombined.putImageData(imageDataCombined, 0, 0);


	context2.clearRect(0, 0, canvas2.width, canvas2.height);
	context2.drawImage(videoElement2, 0, 0, canvas2.width, canvas2.height);
	const imageData2 = context2.getImageData(0, 0, canvas2.width, canvas2.height);
	const dataGreenBlue = imageData2.data;

	for (let i = 0; i < dataGreenBlue.length; i += 4) {
		if (document.getElementById("colType").checked == 1)	// orange/blue
		{
			dataCombined[i + 2] = dataGreenBlue[i + 2]; // Blue channel
		}
		else
		{
			dataCombined[i + 1] = dataGreenBlue[i + 1]; // Green channel
			dataCombined[i + 2] = dataGreenBlue[i + 2]; // Blue channel
		}
	}
	context3d.putImageData(imageDataCombined, 0, 0);


	setTimeout(cloneVideoFrame, 1000 / 20); // Adjust the delay as needed
}


// Get the record button
var recordButton = document.getElementById("recordButton");

// Declare variables for recording
var mediaRecorder;
var recordedChunks = [];

// Set up the MediaRecorder when the record button is clicked
/*recordButton.onclick = function() {
  if (recordButton.textContent === "Record") {
    startRecording();
    recordButton.textContent = "Stop";
  } else {
    stopRecording();
    recordButton.textContent = "Record";
  }
};*/

// Start recording the canvas content
function startRecording() {
  // Clear any existing recorded chunks
  recordedChunks = [];

  // Create a stream from the canvas capture stream
  var stream = canvasElement.captureStream(20);

  // Create a new MediaRecorder with the stream and desired MIME type
  mediaRecorder = new MediaRecorder(stream, { mimeType: "video/webm; codecs=vp9" });

  // Listen for data available event and store the recorded chunks
  mediaRecorder.ondataavailable = function(event) {
	  console.log(event);
    if (event.data.size > 0) {
      recordedChunks.push(event.data);
    }
  };

  // Start recording
  mediaRecorder.start();
}

// Stop recording and save the recorded video
function stopRecording() {
  // Stop the MediaRecorder
  mediaRecorder.stop();

  // Create a new Blob from the recorded chunks
  var blob = new Blob(recordedChunks, { type: "video/webm" });

  // Create a temporary URL for the blob
  var url = URL.createObjectURL(blob);

  // Create a download link and simulate a click to trigger the download
  var a = document.createElement("a");
  a.href = url;
  a.download = "recorded_video.webm";
  
  // Add the download link to the document body
  document.body.appendChild(a);

  // Simulate a click to trigger the download
  a.click();

  // Clean up the download link and the temporary URL
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}

