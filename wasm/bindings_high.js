/**
 * Better bindings for web assembly module
 */
class faceReplacer {
  constructor(imageData) {
    this.srcImg = this.mallocVect(imageData.data);
    this.detector = new Module.FaceDetect(true);
    this.replacer = new Module.FaceReplace(this.detector, this.srcImg, imageData.width, imageData.height);
    this.mat = Module.initMat(this.srcImg, imageData.width, imageData.height);
//    this.faces = this.replacer.DetectFaces(this.mat);
//    console.log(this.faces.get(0));
//    console.log(this.faces.size());
//    console.log(this.replacer.DetectLandmarks(this.mat, this.faces.get(0)));
  }

  /**
   * @param imageData imageData from a canvas element
   * @return imageData modified with face replaced
   */
  replaceFace(imageData) {
    let vectInt = this.mallocVect(imageData.data);
    try {
      this.replacer.MapToFace(vectInt, imageData.width, imageData.height);
    } catch(e) {
      console.log(e);
    }

    // Overwrite image data
    const len = vectInt.size();
    for (let j = 0; j < len; j++) {
      imageData.data[j] = vectInt.get(j);
    }

    this.freeVect(vectInt);
    return imageData;
  }

  mallocVect(data) {
    var y = new Module.VectorInt();
    for (let i = 0; i < data.length; i++) {
      y.push_back(data[i]);
    }

    return y;
  }

  freeVect(y) {
    Module._free(y);
  }
}
