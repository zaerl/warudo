<script setup lang="ts">
import { getData, type View } from '@/data/api';
import { onMounted, ref } from 'vue';

let views = ref<View[] | null>([]);
let busy = ref(true);
let invalid = ref(false);
console.log('views', views);

onMounted(async () => {
  views.value = await getData<View[]>('views');
  console.log('views', views);

  busy.value = false;
  invalid.value = views.value === null;
});
</script>

<template>
<main class="container">
  <table :aria-busy="busy" :aria-invalid="invalid">
    <thead>
      <tr>
        <td>#</td>
        <td>Name</td>
      </tr>
    </thead>
    <tbody>
      <tr v-if="invalid || !views">
        <td colspan="2">Can't catch data from server</td>
      </tr>
      <tr v-else-if="!views.length">
        <td colspan="2">No results</td>
      </tr>
      <tr v-else v-for="view in views">
        <td>{{ view.id }}</td>
        <td>{{ view.name }}</td>
      </tr>
    </tbody>
  </table>
</main>
</template>
