#ifndef ModuleRules_h
#define ModuleRules_h

struct RulesConfig {
  long refreshTime = 1000 * 10;  // 10 Segundos
  long lastRefreshTime = 0;
} rulesConfig;

void applySensors() {
  if(millis() - rulesConfig.lastRefreshTime >= rulesConfig.refreshTime || millis() - rulesConfig.lastRefreshTime < 0) {

    // Do something

    // Actualizo el tiempo
    rulesConfig.lastRefreshTime = millis();
  }
}

#endif
