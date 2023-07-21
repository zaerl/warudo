<script setup lang="ts">

interface Filter {
  id: number,
  key: string,
  value: string | number | boolean,
}

interface Props {
  filters?: Filter[],
}

const props = withDefaults(
  defineProps<Props>(), {
    filters: () => [],
  }
);

const emit = defineEmits(['selectFilter']);

function selectFilter(index: number) {
  emit('selectFilter', index);
}
</script>

<template>
<a
  v-for="(filter, index) in props.filters"
  v-bind:key="filter.id"
  @click.prevent="selectFilter(index)"
  href=""
  role="button"
  class="contrast outline filter">{{ filter.key }}: {{ filter.value }}</a>
</template>

<style scoped>
.filter {
  margin-right: calc(var(--spacing) * .5);
}
</style>
