const FACES = [
  'brando.jpg',
  'busey.jpg',
  'dmx.jpg',
  'efron.jpg',
  'foxx.jpg',
  'harvey.jpg',
  'murphy.png',
  'nixon.jpg',
  'obama.png',
  'savage.jpg',
  'snoopp.jpg',
  'spacey.jpg',
  'welles.jpg',
];

const video = document.getElementById('video');
const videoOut = document.getElementById('video-out');
let isStreaming = false;
let height = 300;
let width = 300;

video.addEventListener('canplay', (e) => {
  if (!isStreaming) {
    // videoWidth isn't always set correctly in all browsers
    if (video.videoWidth > 0) {
      height = video.videoHeight / (video.videoWidth / width);
    }

    videoOut.setAttribute('width', width);
    videoOut.setAttribute('height', height);

    // Reverse the canvas image
    let con = videoOut.getContext('2d');
    con.translate(width, 0);
    con.scale(-1, 1);
    isStreaming = true;
  }
}, false);


video.addEventListener('play', () => {
  // Every 33 milliseconds copy the video image to the canvas
  setInterval(()  => {
    if (video.paused || video.ended) {
      return;
    }
    let con = videoOut.getContext('2d');
    con.fillRect(0, 0, width, height);
    con.drawImage(video, 0, 0, width, height);
  }, 200);
}, false);

function loadImageFile(url) {
  var image = new Image();
  image.src = url;
  return new Promise((accept, reject) => {
    image.onload = accept;
    image.onerror = reject;
  }).then(accept => {
    srcWidth = image.width;
    srcHeight = image.height;
    let faceCanvas = document.getElementById('face-canvas');
    let faceCon = faceCanvas.getContext('2d');
    faceCanvas.width = image.width;
    faceCanvas.height = image.height;
    faceCon.clearRect(0, 0, image.width, image.height);
    faceCon.drawImage(image, 0, 0);
  });
}

function startWorker() {
  const srcCanvas = videoOut;
  const srcContext = videoOut.getContext('2d');
  const targetCanvas = document.getElementById('output');
  const targetContext = targetCanvas.getContext('2d');
  const name = FACES[Math.floor(Math.random()*FACES.length)];
  loadImageFile(`/static/faces/${name}`).then(() => {
    const worker = new Worker('/wasm/worker.js');
    worker.onmessage = function(e) {
      switch (e.data.msg) {
      case 'bootedUp':
        const faceCanvas = document.getElementById('face-canvas');
        const faceCon = faceCanvas.getContext('2d');
        worker.postMessage({ msg: 'init', image: faceCon.getImageData(0, 0, faceCanvas.width, faceCanvas.height)});
        document.getElementById('loading').innerHTML = 'Performing setup';
        break;
      case 'init':
        document.getElementById('loading').style.display = 'none';
        worker.postMessage({ msg: 'replace', image: srcContext.getImageData(0, 0, srcCanvas.width, srcCanvas.height)});
        break;
      case 'replaced':
        worker.postMessage({ msg: 'replace', image: srcContext.getImageData(0, 0, srcCanvas.width, srcCanvas.height)});
        let im = e.data.image;
        targetContext.clearRect(0, 0, im.width, im.height);
        targetContext.putImageData(im, 0, 0);
        break;
      }
    };
  });
}

navigator.mediaDevices.getUserMedia({ video: true }).then((stream) => {
  const url = window.URL || window.webkitURL;
  video.src = url ? url.createObjectURL(stream) : stream;
  video.play();
  startWorker();
}).catch((error) => {
  console.log(error);
  let helpTxt = document.getElementById('loading');
  helpTxt.innerHTML = 'Unfortunately it looks like you do not have a camera hooked up. Sadly this means you can\'t experience all this cool stuff. You should plug one in!';
});
