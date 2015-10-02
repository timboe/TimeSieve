
// This is not designed to be any real security - but will mask null regions of the save byestream
var key = [189, 212, 188, 59, 119, 196, 5, 153, 50, 247, 93, 162, 159, 58, 216, 181, 40, 23, 53, 89, 21, 124, 65, 67, 57, 92, 87, 154, 78, 198, 253, 2, 74, 77, 16, 194, 46, 219, 94, 29, 56, 76, 144, 243, 161, 218, 19, 220, 107, 111, 69, 118, 217, 61, 26, 63, 173, 60, 142, 104, 102, 14, 27, 109];

var weatherEnum = {
  CLEAR_DAY: 0,
  CLEAR_NIGHT: 1,
  LOW_CLOUD_DAY: 2,
  LOW_CLOUD_NIGHT: 3,
  MED_CLOUD: 4,
  HIGH_CLOUD: 5,
  LOW_RAIN: 6,
  HIGH_RAIN: 7,
  THUNDER: 8,
  SNOW: 9,
  MIST: 10,
  WEATHER_NA: 11
}

var mapWeather = {
  '01d': weatherEnum.CLEAR_DAY,
  '01n': weatherEnum.CLEAR_NIGHT,
  '02d': weatherEnum.LOW_CLOUD_DAY,
  '02n': weatherEnum.LOW_CLOUD_NIGHT,
  '03d': weatherEnum.MED_CLOUD,
  '03n': weatherEnum.MED_CLOUD,
  '04d': weatherEnum.HIGH_CLOUD,
  '04n': weatherEnum.HIGH_CLOUD,
  '09d': weatherEnum.LOW_RAIN,
  '09n': weatherEnum.LOW_RAIN,
  '10d': weatherEnum.HIGH_RAIN,
  '10n': weatherEnum.HIGH_RAIN,
  '11d': weatherEnum.THUNDER,
  '11n': weatherEnum.THUNDER,
  '13d': weatherEnum.SNOW,
  '13n': weatherEnum.SNOW,
  '50d': weatherEnum.MIST,
  '50n': weatherEnum.MIST,
}

var LOCAL_DATA_KEY = 10;

function crypt(bytes) {
  for (var out = [], i = 0; i < bytes.length; i++) {
    var loc = i;
    while (loc > 63) loc -= 64;
    out.push(bytes[i] ^ key[loc]);
  }
  return out;
}

function bytesToHex(bytes, version) {
  var hex = [];
  if (version < 16) hex.push("0"); //Padding
  hex.push(version.toString(16));
  for (i = 0; i < bytes.length; i++) {
    if (bytes[i] < 16) hex.push("0"); //Padding
    hex.push(bytes[i].toString(16));
  }
  return hex.join("");
}

function hexToBytes(hex) {
  // Skip the first two as that's the schema
  for (var bytes = [], c = 2; c < hex.length; c += 2)
    bytes.push(parseInt(hex.substr(c, 2), 16));
  return bytes;
}

function hexToSchema(hex) {
  return parseInt(hex.substr(0, 2), 16);
}

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

Pebble.addEventListener('ready',
  function(e) {
    // Any saved data?
    var localSave = localStorage.getItem(LOCAL_DATA_KEY);
    console.log('JS: Ready event. Local save is :' + localSave);
    getWeather();
  }
);

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://tim-martin.co.uk/timesink/configuration/index.html';
  var localSave = localStorage.getItem(LOCAL_DATA_KEY);
  if (localSave) url += "?backup=" + localSave;
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

  // Did we get a save to load?
  if ('op_loadSave' in configData && configData['op_loadSave'] != "") {
    var loadData = configData[op_loadSave];
    //Check is hex
    if (loadData.matches("[0-9A-F]+") == true) {
      var decodedSave = hexToBytes(loadData);
      var decodedSchema = hexToSchema(loadData);
      if (decodedSchema == 1) {
        if (loadData.length == 354) {
          // OK! we load the save
          dict['KEY_SAVE_VERSION'] = 1;
          dict['KEY_SAVE_DATA'] = crypt(decodedSave);
        } else {
          console.log('JS: ERROR: Wrong size for schema 1:' + loadData.length);
        }
      } else {
        console.log('JS: ERROR: Unknown schema ' + decodedSchema);
      }
    } else {
      console.log('JS: ERROR: Not hex');
    }
  }

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('JS: Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('JS: Send failed!');
  });

});

Pebble.addEventListener('appmessage', function(e) {

  console.log('JS: Got an AppMessage: ' + JSON.stringify(e.payload));

  var schema = e.payload['KEY_SAVE_VERSION'];
  var rawSave = e.payload['KEY_SAVE_DATA'];
  var weatherOn = e.payload['KEY_WEATHER_ON'];

  // Was there save data?
  if (schema && rawSave) {
    var cypherSave = crypt(rawSave);
    var encodedSave = bytesToHex(cypherSave, schema);
    localStorage.setItem(LOCAL_DATA_KEY, encodedSave);

    // Check we can decode too
    var decodedSave = hexToBytes(encodedSave);
    var decodedSchema = hexToSchema(encodedSave);
    var decypherSave = crypt(decodedSave);

    if (decodedSchema != schema || JSON.stringify(decypherSave) != JSON.stringify(rawSave)) {
      console.log('JS: There was an error undoing the save mangling! got back Schema:' + decodedSchema + " data:" + decypherSave);
    } else {
      console.log('JS: Save encode/decode check passed');
    }
  }

  // Did this message include a request to update the weather?
  if (weatherOn) {
    getWeather();
  }

});

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET',
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log('JS: Temperature in ' + json.name + '  is ' + temperature);

      var weatherCode = json.weather[0].icon;
      var icon = weatherEnum.WEATHER_NA;
      if (weatherCode in mapWeather) icon = mapWeather[weatherCode];
      console.log('JS: Conditions are code:' + weatherCode + " icon ID:" + icon);

      var dict = {};
      dict['KEY_WEATHER_TEMP'] = temperature;
      dict['KEY_WEATHER_ICON'] = icon;

      // Send to watchapp
      Pebble.sendAppMessage(dict, function() {
        console.log('JS: Weather send successful: ' + JSON.stringify(dict));
      }, function() {
        console.log('JS: Weather send failed!');
      });

    }
  );
}

function locationError(err) {
  console.log('JS: Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}
