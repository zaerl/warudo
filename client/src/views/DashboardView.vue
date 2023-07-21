<script setup lang="ts">
import MainGrid from '@/components/MainGrid.vue'
import SearchBar from '@/components/search/SearchBar.vue'
import { getData, type Dashboard } from '@/data/api';
import { onMounted, ref } from 'vue';
import { useRoute } from 'vue-router';
// import { useRoute } from 'vue-router';

const route = useRoute();
const id = parseInt(route.params.id as string);

let entry = ref<Dashboard | null>();
let busy = ref(true);
let invalid = ref(false);

onMounted(async () => {
  const dashboards = await getData<Dashboard[]>('dashboards', { id: id });

  if(dashboards && dashboards.length && dashboards[0]) {
    entry.value = dashboards[0];
  } else {
    entry.value = null;
  }

  busy.value = false;
  invalid.value = entry.value === null;
  document.title = route.meta.title + ' - ' + entry.value?.data.name;
});
</script>

<template>
<SearchBar />
<MainGrid :dashboard="entry" :busy="busy" :invalid="invalid" />
</template>
