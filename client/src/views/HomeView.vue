<script setup lang="ts">
import { getData, type Entry, type View } from '@/data/api';
import { onMounted, ref } from 'vue';

let entries = ref<any[] | null>([]);
let busy = ref(true);
let invalid = ref(false);

onMounted(async () => {
  entries.value = await getData<any[]>();

  busy.value = false;
  invalid.value = entries.value === null;
});
</script>

<template>
<main class="container-fluid">
  <table :aria-busy="busy" :aria-invalid="invalid">
    <thead>
      <tr>
        <td>Data</td>
      </tr>
    </thead>
    <tbody>
      <tr v-if="invalid || !entries">
        <td colspan="1">Can't catch data from server</td>
      </tr>
      <tr v-else-if="!entries.length">
        <td colspan="1">No results</td>
      </tr>
      <tr v-else v-for="entry in entries">
        <td>
          <samp><span v-for="(value, key, index) in entry"><span v-if="index !== 0">, </span><strong>{{ key }}</strong>: {{ value }}</span></samp>
        </td>
      </tr>
    </tbody>
  </table>
</main>
</template>
