<script setup lang="ts">
defineEmits(['add']);
</script>

<template>
<a href="" class="secondary add" role="button" @click.prevent="$emit('add')">
  <svg xmlns="http://www.w3.org/2000/svg" height="16px" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
    <line x1="12" y1="5" x2="12" y2="19"></line>
    <line x1="5" y1="12" x2="19" y2="12">'</line>
  </svg>
  Add filter
</a>
</template>

<style scoped>
a.add {
  margin-bottom: var(--spacing);
  margin-right: calc(var(--spacing) * .5);
}
a.add svg {
  stroke: var(--secondary);
  margin-right: 0.5rem;
  border: 2px solid currentColor;
  border-radius: 1rem;
  background: currentColor;
  vertical-align: unset;
}
</style>
