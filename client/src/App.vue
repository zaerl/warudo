<script setup lang="ts">
import { RouterView } from 'vue-router';
import PageFooter from './components/PageFooter.vue';
import PageNav from './components/PageNav.vue';
import { setMainTheme } from './data/theme';
import { useSettingsStore } from './stores/settings';

const settingsStore = useSettingsStore();

setMainTheme(settingsStore.settings.preferredTheme);
</script>

<template>
  <PageNav name="Warudo Dashboard" />
  <RouterView />
  <PageFooter />
</template>

<style scoped></style>
