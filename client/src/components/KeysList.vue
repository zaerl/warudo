<script setup lang="ts">
import type { GetDataType } from '@/data/api';
import { getData, type Key } from '@/data/api';
import { onMounted, ref } from 'vue';

interface Props {
  type: GetDataType,
}

const props = defineProps<Props>();

let busy = ref(true);
let invalid = ref(false);
let keys = ref<Key[] | null>([]);

onMounted(async () => {
  keys.value = await getData<Key[]>(props.type);

  busy.value = false;
  invalid.value = keys.value === null;
});
</script>

<template>
<select v-if="invalid" :aria-invalid="invalid" id="keys-list">
  <option>Can't catch data from server</option>
</select>
<select v-else :aria-busy="busy" id="keys-list">
  <option v-bind:key="key.name" v-for="key in keys" :value="key.name">{{ key.name }} ({{ key.value }})</option>
</select>
</template>

<style scoped></style>
