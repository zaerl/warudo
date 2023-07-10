<script setup lang="ts">
import { ref, type Ref } from 'vue';
import Footer from './components/Footer.vue';
import MainGrid from './components/MainGrid.vue';
import Nav from './components/Nav.vue';
import Search from './components/Search.vue';

let topLinks: Ref<string[]> = ref([]);

topLinks.value = ['Settings'];

</script>

<template>
  <Nav name="Warudo Dashboard" :links="topLinks" />
  <Search />
  <MainGrid />
  <Footer />
</template>

<style scoped>

</style>
