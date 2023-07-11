import type { DocumentTheme } from "./theme";

export interface Settings {
  preferredTheme: DocumentTheme;
}

export function getDefaultSettings(): Settings {
  return {
    preferredTheme: 'auto',
  };
}

export function getSettings(): Settings {
  const loaded = localStorage.getItem('settings');

  if(loaded) {
    const parsed = JSON.parse(loaded);

    if(parsed) {
      return parsed;
    }
  }

  return getDefaultSettings();
}

export function saveSettings(settings: Settings) {
  localStorage.setItem('settings', JSON.stringify(settings));
}
