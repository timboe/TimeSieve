#include <pebble.h>
#include "communication.h"
#include "persistence.h"
#include "constants.h"
#include "timeSink.h"

void inboxReceiveHandler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "gotMsg");

  Tuple* reset = dict_find(iter, KEY_RESET);
  if (reset && reset->value->int32 > 0) resetUserData();

  Tuple* save =  dict_find(iter, KEY_SAVE_DATA);
  Tuple* schema = dict_find(iter, KEY_SAVE_VERSION);
  if (save && schema) {
    APP_LOG(APP_LOG_LEVEL_INFO, "LoadSave - TODO code this");
  }

  Tuple* animation = dict_find(iter, KEY_ANIMATION);
  if (animation) setUserOpt(OPT_ANIMATE, (animation->value->int32 > 0));

  Tuple* show_seconds = dict_find(iter, KEY_SHOW_SEC);
  if (show_seconds) setUserOpt(OPT_SHOW_SECONDS, (show_seconds->value->int32 > 0));

  Tuple* temp_celsius = dict_find(iter, KEY_TEMP_CELSIUS);
  if (temp_celsius) setUserOpt(OPT_CELSIUS, (temp_celsius->value->int32 > 0));

  Tuple* quiet_start = dict_find(iter, KEY_QUIET_START);
  if (quiet_start) setUserSetting(SETTING_ZZZ_START, quiet_start->value->int32);

  Tuple* quiet_end = dict_find(iter, KEY_QUIET_END);
  if (quiet_end) setUserSetting(SETTING_ZZZ_END, quiet_end->value->int32);

  Tuple* temp = dict_find(iter, KEY_WEATHER_TEMP);
  Tuple* icon = dict_find(iter, KEY_WEATHER_ICON);
  if (temp && icon) updateWeather(temp->value->int32, icon->value->int32);

}

void inboxRecieveFailed(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "msgGet Failed! Code %i", (int)reason);
}

void outboxSendOK(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "msgSend OK.");
}

void outboxSendFailed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "msgSend Failed! Code %i", (int)reason);
}

/**
 * Send the current save to the phone where it may be displayed to the user.
 */
void sendStateToPhone() {
  DictionaryIterator *iter;

  app_message_outbox_begin(&iter);
  DictionaryResult resultD = dict_write_data(iter, KEY_SAVE_DATA, (const uint8_t*) getData(), sizeof(struct userData_v1));
  DictionaryResult resultV = dict_write_uint8(iter, KEY_SAVE_VERSION, SCHEMA_VERSION);
  DictionaryResult resultW = dict_write_uint8(iter, KEY_WEATHER_ON, (getUserSetting(SETTING_TECH) >= TECH_WEATHER ? 1 : 0));
  uint32_t bytesToWrite = dict_write_end(iter);

  if (resultD != DICT_OK || resultV != DICT_OK || resultW != DICT_OK) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Dict cr failed. Codes d:%i v:%i w:%i", (int)resultD, (int)resultV, (int)resultW);
  } else {
    app_message_outbox_send(); // Bon voyage!
    APP_LOG(APP_LOG_LEVEL_DEBUG, "save sent to phone! %i bytes", (int)bytesToWrite);
  }
}

void requestWeatherUpdate() {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, KEY_WEATHER_ON, 1);
  app_message_outbox_send();
}

void registerCommunication() {
  static bool isDone = false;
  if (isDone == true) return;

  app_message_register_inbox_received(inboxReceiveHandler);
  app_message_register_inbox_dropped(inboxRecieveFailed);
  app_message_register_outbox_sent(outboxSendOK);
  app_message_register_outbox_failed(outboxSendFailed);

  // TODO bring this down to save space
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  isDone = true;
}

void destroyCommunication() {
  app_message_deregister_callbacks();
}
