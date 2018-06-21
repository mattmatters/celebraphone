module.exports = {
  'homepage' : function (browser) {
    browser
      .url(browser.launch_url)
      .waitForElementVisible('body', 1000)
      .pause(1000)
      .assert.containsText('#title', 'Talk with a Celebrity')
      .end();
  }
};
