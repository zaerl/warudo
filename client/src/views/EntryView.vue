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

onMounted(async () => {
  const entries = await getData<Entry[]>('entries', { id: id });

  if(entries && entries.length && entries[0]) {
    entry.value = entries[0];
  } else {
    entry.value = null;
  }

  busy.value = false;
  invalid.value = entry.value === null;
});
</script>

<template>
<main class="container">
<div>
  <div v-if="!entry && !busy">Can't catch data from server</div>
  <JSONBlock v-if="entry" :json="entry?.data" />
</div>
</main>
</template>
