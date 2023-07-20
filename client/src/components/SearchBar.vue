<script setup lang="ts">
import { ref } from 'vue';
import ModalDialog from './ModalDialog.vue';

interface Filter {
  id: number,
  key: string,
  value: string | number | boolean,
}

interface Props {
  modelValue?: string,
}

const props = withDefaults(
  defineProps<Props>(), {
    modelValue: '',
  }
);

let filters = ref<Filter[]>([]);
let isModalOpen = ref(false);

defineEmits(['update:modelValue']);

function onClose(confirmed: boolean) {
  isModalOpen.value = false;

  if(confirmed) {
    filters.value.push({
      id: filters.value.length + 1,
      key: 'key',
      value: 'value',
    });
  }
}

function addFilter() {
  isModalOpen.value = true;
}

function selectFilter(index: number) {
  addFilter();
}
</script>

<template>
<ModalDialog
  title="Add filter"
  confirm-button="Add"
  :is-open="isModalOpen"
  @close="onClose">
hello
</ModalDialog>
<div id="search" class="container-fluid">
  <input
    type="search"
    id="search"
    name="search"
    placeholder="Search"
    :value="props.modelValue"
    @input="$emit('update:modelValue', ($event?.target as HTMLInputElement)?.value)" />
  <a href="" class="secondary add" role="button" @click.prevent="addFilter">
    <svg xmlns="http://www.w3.org/2000/svg" height="16px" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="3" stroke-linecap="round" stroke-linejoin="round">
      <line x1="12" y1="5" x2="12" y2="19"></line>
      <line x1="5" y1="12" x2="19" y2="12">'</line>
    </svg>
    Add filter
  </a>
  <a
    v-for="(filter, index) in filters"
    v-bind:key="filter.id"
    @click.prevent="selectFilter(index)"
    href=""
    role="button"
    class="contrast outline filter">{{ filter.key }}: {{ filter.value }}</a>
</div>
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

.filter {
  margin-right: calc(var(--spacing) * .5);
}
</style>
