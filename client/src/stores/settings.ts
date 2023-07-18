import type { DocumentTheme } from '@/data/theme';
import { defineStore, acceptHMRUpdate } from 'pinia';
import { ref } from 'vue';

export interface Settings {
    preferredTheme: DocumentTheme;
  }

export const useSettingsStore = defineStore('settings', () => {
  let parsed: Settings | null = null;
  const defaultSettings: Settings = {
    preferredTheme: 'auto',
  };

  function save() {
    localStorage.setItem('settings', JSON.stringify(settings.value));
  }

  try {
    let loaded = localStorage.getItem('settings');
    parsed = loaded ? JSON.parse(loaded) : null;
  } catch(e) {
    console.warn('Failed to parse settings from localStorage');
  }

  const settings = ref<Settings>({ ...defaultSettings, ...parsed });

  return {
    settings,
    save,
  };
});

if(import.meta.hot) {
  import.meta.hot.accept(acceptHMRUpdate(useSettingsStore, import.meta.hot));
}
