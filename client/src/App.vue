<script setup lang="ts">
import { RouterView } from 'vue-router'
import Footer from './components/Footer.vue'
import Nav from './components/Nav.vue'
</script>

<template>
  <Nav name="Warudo Dashboard" />
  <RouterView />
  <Footer />
</template>

<style scoped></style>
