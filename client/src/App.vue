<script setup lang="ts">
import { ref } from 'vue';
import { RouterView } from 'vue-router';
import Footer from './components/Footer.vue';
import Nav from './components/Nav.vue';
import { getSettings } from './data/settings';
import { setMainTheme } from './data/theme';

const settings = ref(getSettings());

setMainTheme(settings.value.preferredTheme);

</script>

<template>
  <Nav name="Warudo Dashboard" />
  <RouterView />
  <Footer />
</template>

<style scoped></style>
