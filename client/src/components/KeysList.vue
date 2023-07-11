<script setup lang="ts">
import { getData } from '@/data/api'
import { onMounted, ref } from 'vue'

interface Key {
  [key: string]: number
}

let busy = ref(true);
let invalid = ref(false);
let keys = ref<Key[] | null>([]);

onMounted(async () => {
  keys.value = await getData<Key[]>('keys')

  busy.value = false
  invalid.value = keys.value === null
});
</script>

<template>
<div id="main-keys-box" :aria-busy="busy" :aria-invalid="invalid" class="box">
  <div v-if="invalid">Can't catch data from server</div>
  <select v-else id="main-keys">
    <option v-bind:key="key.name" v-for="key in keys" :value="key.name">{{ key.name }} ({{ key.count }})</option>
  </select>
</div>
</template>

<style scoped></style>
