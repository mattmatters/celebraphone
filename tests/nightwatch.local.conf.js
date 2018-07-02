const nightwatch_config = {
  src_folders: ['tests'],
  output_folder: 'reports',
  selenium : {
    "start_process" : false,
    "host" : "hub-cloud.browserstack.com",
    "port" : 80
  },

  test_settings: {
    default: {
      launch_url: 'http://' + (process.env.ADDRESS || 'localhost') + ':80',
      desiredCapabilities: {
        browser: 'chrome',
        project: 'celebraphone',
        build: process.env.CI_PIPELINE_ID || 'test',
        'browserstack.user': process.env.BROWSERSTACK_USER,
        'browserstack.key': process.env.BROWSERSTACK_ACCESS_KEY,
        'browserstack.local': true,
      }
    },
    chrome_desktop_67: {
      desiredCapabilities: {
        os: 'OS X',
        os_version: 'High Sierra',
        browser: 'chrome',
        browserName: 'chrome',
        browser_version: '67.0',
        'browserstack.debug': true,
        'browserstack.selenium_version' : '3.5.2',
      }
    },
    chrome_android_latest: {
      desiredCapabilities: {
        'os_version' : '7.0',
        'device' : 'Samsung Galaxy S8',
        'real_mobile' : 'true',
        'browserstack.local' : 'true',
        'browserstack.debug' : 'true',
        'browserstack.appium_version' : '1.7.2',
      }
    },
    iphone_8: {
      desiredCapabilities: {
        'os_version' : '11.0',
        'device' : 'iPhone 8',
        'real_mobile' : 'true',
        'browserstack.local' : 'true',
        'browserstack.debug' : 'true',
        'browserstack.appium_version' : '1.7.0',
      }
    },
    firefox_desktop_58: {
      desiredCapabilities: {
        browser: "firefox",
        browser_version: '58.0',
      }
    },
    safari: {
      desiredCapabilities: {
        browser: "safari"
      }
    },
    ie: {
      desiredCapabilities: {
        browser: "internet explorer"
      }
    },
  }
};

// Code to copy seleniumhost/port into test settings
for(var i in nightwatch_config.test_settings){
  var config = nightwatch_config.test_settings[i];
  config['browserstack.user'] = process.env.BROWSERSTACK_USER;
  config['browserstack.key'] = process.env.BROWSERSTACK_ACCESS_KEY;
  config['browserstack.local'] = true;
  config['selenium_host'] = nightwatch_config.selenium.host;
  config['selenium_port'] = nightwatch_config.selenium.port;
}

module.exports = nightwatch_config;
