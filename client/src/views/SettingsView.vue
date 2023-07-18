<script setup lang="ts">
import { setMainTheme } from '@/data/theme';
import { useSettingsStore } from '@/stores/settings';

const settingsStore = useSettingsStore();

function save() {
  settingsStore.save();
  setMainTheme(settingsStore.settings.preferredTheme);
}
</script>

<template>
<main class="container">
  <h1>Settings</h1>
  <article aria-label="settings">
    <form>
      <div class="grid">
        <label for="theme">Theme
          <select name="theme" id="theme" v-model="settingsStore.settings.preferredTheme">
            <option value="auto">Sync with system</option>
            <option value="light">Light</option>
            <option value="dark">Dark</option>
          </select>
        </label>
      </div>
      <div class="settings-actions">
        <RouterLink to="/" role="button" class="outline">Cancel</RouterLink>
        <a href="#" role="button" @click="save">Save</a>
      </div>
    </form>
  </article>
</main>
</template>

<style scoped>
form {
  margin-bottom: 0;
}

.settings-actions {
  text-align: right;
}

.settings-actions a[role=button] {
  margin-left: calc(var(--spacing) * 0.5);
}

</style>
