<script setup lang="ts">
import { ref } from 'vue';

let ariaBusy = ref(true);
</script>

<template>
<div id="main-keys-box" :aria-busy="ariaBusy" class="box">
  <select id="main-keys"></select>
</div>
</template>

<style scoped></style>
