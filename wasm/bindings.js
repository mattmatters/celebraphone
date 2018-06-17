/**
 * Better bindings for web assembly module
 */
class faceReplacer {
  constructor(imageData) {
    this.srcImg = this.mallocVect(imageData.data);
    this.replacer = new Module.FaceReplace(this.srcImg, imageData.width, imageData.height);
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
