<script setup lang="ts">
import { ref } from 'vue';
import ModalDialog from '../ModalDialog.vue';
import SearchAddFilter from './SearchAddFilter.vue';
import SearchFilters from './SearchFilters.vue';

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

function selectFilter(_index: number) {
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
  <SearchAddFilter @add="addFilter" />
  <SearchFilters
    :filters="filters"
    @select-filter="selectFilter" />
</div>
</template>
