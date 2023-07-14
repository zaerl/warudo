<script setup lang="ts">
import MainGrid from '@/components/MainGrid.vue'
import SearchBar from '@/components/SearchBar.vue'
import { getData, type View } from '@/data/api';
import { onMounted, ref } from 'vue';
import { useRoute } from 'vue-router';
// import { useRoute } from 'vue-router';

const route = useRoute();
const id = parseInt(route.params.id as string);

let entry = ref<View | null>();
let busy = ref(true);
let invalid = ref(false);

onMounted(async () => {
  const views = await getData<View[]>('views', { id: id });

  if(views && views.length && views[0]) {
    entry.value = views[0];
  } else {
    entry.value = null;
  }

  busy.value = false;
  invalid.value = entry.value === null;
});

</script>

<template>
<SearchBar />
<MainGrid :view="entry" :busy="busy" :invalid="invalid" />
</template>
