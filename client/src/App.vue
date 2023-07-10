<script setup lang="ts">
import { onMounted, ref, type Ref } from 'vue';
import Footer from './components/Footer.vue';
import MainGrid from './components/MainGrid.vue';
import Nav from './components/Nav.vue';
import Search from './components/Search.vue';
import { getData } from './data/api';

interface Key {
  [key: string]: number;
}

let topLinks: Ref<string[]> = ref([]);
let keys = ref<Key[] | null>([]);

topLinks.value = ['Settings'];

onMounted(async () => {
  keys.value = await getData<Key[]>('keys');
});
</script>

<template>
  <Nav name="Warudo Dashboard" :links="topLinks" />
  <Search />
  <MainGrid />
  <Footer />
</template>

<style scoped>

</style>
