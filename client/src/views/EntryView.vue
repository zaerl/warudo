<script setup lang="ts">
import JSONBlock from '@/components/JSONBlock.vue';
import { getData, type Entry } from '@/data/api';
import { onMounted, ref } from 'vue';
import { useRoute } from 'vue-router';

const route = useRoute();
const id = parseInt(route.params.id as string);

let entry = ref<Entry | null>();
let busy = ref(true);
let invalid = ref(false);
let json = "";

onMounted(async () => {
  const entries = await getData<Entry[]>('entries', { id: id });
  let addComma = false;
  json += "{";

  if(entries && entries.length && entries[0]) {
    entry.value = entries[0];

    for(const property in entry.value.data) {
      if(addComma) {
        json += ",\n";
      } else {
        addComma = true;
        json += "\n";
      }

      const value = entry.value.data[property];
      json += `  "${property}": `;

      switch(typeof value) {
        case 'string':
          json += `"${value}"`;
          break;
        case 'number':
          json += `${value}`;
          break;
        case 'boolean':
          json += `${value}`;
          break;
        default:
          json += `"${value}"`;
          break;
      }
    }
  } else {
    entry.value = null;
  }

  json += "\n}";
  busy.value = false;
  invalid.value = entry.value === null;
});
</script>

<template>
<main class="container">
<div :aria-busy="busy" :aria-invalid="invalid">
  <div v-if="!entry">Can't catch data from server</div>
  <JSONBlock v-else :json="entry.data" />
</div>
</main>
</template>
