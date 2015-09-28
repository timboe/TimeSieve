Pebble.addEventListener('ready',
  function(e) {
    console.log('JS: Ready event');
  }
);

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://tim-martin.co.uk/timesink/configuration/index.html';
  console.log('JS: Open conf page ' + url);
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('JS: Conf page returned: ' + JSON.stringify(configData));

  var dict = {};
  dict['KEY_ANIMATION']    = configData['op_animation'];
  dict['KEY_TEMP_CELSIUS'] = configData['op_celcius'];
  dict['KEY_SHOW_SEC']     = configData['op_seconds'];
  dict['KEY_QUIET_START']  = parseInt(configData['op_quietStart']);
  dict['KEY_QUIET_END']    = parseInt(configData['op_quietEnd']);
  dict['KEY_RESET']        = configData['op_reset'];

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('JS: Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('JS: Send failed!');
  });

});
