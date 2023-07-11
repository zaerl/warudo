<script setup lang="ts">
import { ref } from 'vue';
import { RouterView } from 'vue-router';
import PageFooter from './components/PageFooter.vue';
import PageNav from './components/PageNav.vue';
import { getSettings } from './data/settings';
import { setMainTheme } from './data/theme';

const settings = ref(getSettings());

setMainTheme(settings.value.preferredTheme);

</script>

<template>
  <PageNav name="Warudo Dashboard" />
  <RouterView />
  <PageFooter />
</template>

<style scoped></style>
