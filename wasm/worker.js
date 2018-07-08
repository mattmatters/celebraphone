var Module = {
  locateFile: (url) => `/wasm/${url}`,
};

importScripts('/wasm/face_replace_high.js', '/wasm/bindings.js');

let replacer;

Module.addOnPostRun(() => {
  postMessage({msg: 'bootedUp'});
});

onmessage = function(e) {
  let res = {};
  switch (e.data.msg) {
  case 'init':
    replacer = new faceReplacer(e.data.image);
    console.log('ok');
    res.msg = 'init';
    break;
  case 'replace':
    res.image = replacer.replaceFace(e.data.image);
    res.msg = 'replaced';
    break;
  }
  postMessage(res);
};
