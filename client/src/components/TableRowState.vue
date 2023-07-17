<script setup lang="ts">
interface Props {
  busy: boolean,
  invalid: boolean,
  count: number,
  columns: number,
}

const props = defineProps<Props>();
</script>

<template>
<tr v-if="busy">
  <td :colspan="props.columns" :aria-busy="props.busy"></td>
</tr>
<tr v-if="invalid">
  <td :colspan="props.columns" :aria-invalid="props.busy">Can't catch data from server</td>
</tr>
<tr v-if="!props.busy && !props.invalid && !props.count">
  <td class="no-results" :colspan="props.columns">No results</td>
</tr>
</template>

<style scoped>
td.no-results {
  text-align: center;
}
</style>
