import type { DocumentTheme } from "./theme";

export interface Settings {
  preferredTheme: DocumentTheme;
}

let userSettings: Settings | null = null;

export function getDefaultSettings(): Settings {
  return {
    preferredTheme: 'auto',
  };
}

export function getSettings(): Settings {
  if(userSettings !== null) {
    return userSettings;
  }

  const loaded = localStorage.getItem('settings');

  if(loaded) {
    const parsed = JSON.parse(loaded);

    if(parsed) {
      userSettings = parsed;

      return parsed;
    }
  }

  userSettings = getDefaultSettings();

  return userSettings;
}

export function saveSettings(settings: Settings) {
  userSettings = settings;
  localStorage.setItem('settings', JSON.stringify(settings));
}
