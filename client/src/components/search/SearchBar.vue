<script setup lang="ts">
import { computed, ref } from 'vue';
import ModalDialog from '../ModalDialog.vue';
import SearchAddFilter from './SearchAddFilter.vue';
import SearchFilters from './SearchFilters.vue';
import KeysList from '../KeysList.vue';

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
let currentFilter = ref<Filter>();

defineEmits(['update:modelValue']);

function addFilter() {
  currentFilter.value = {
    id: filters.value.length + 1,
    key: '',
    value: '',
  }

  isModalOpen.value = true;
}

function selectFilter(_index: number) {
  addFilter();
}

// ModalDialog
const isModalDisabled = computed(() => {
  const ret = currentFilter?.value?.key === '' || currentFilter?.value?.value === '';

  return ret ? true : null;
});

// ModalDialog
function onClose(confirmed: boolean) {
  isModalOpen.value = false;

  if(confirmed && currentFilter?.value?.key && currentFilter.value.value) {
    filters.value.push(currentFilter.value);
  } else {
    currentFilter.value = undefined;
  }
}
</script>

<template>
<ModalDialog
  v-if="currentFilter"
  title="Add filter"
  confirm-button="Add"
  :is-open="isModalOpen"
  :is-disabled="isModalDisabled"
  @close="onClose">
  <form>
    <label for="key">
      Key name
      <KeysList type="keys" v-model="currentFilter.key" />
      <small>The JSON query syntax is supported. See <a target="_blank" href="https://sqlite.org/json1.html#the_json_extract_function">json_extract</a>.</small>
    </label>
    <label for="key">
      Value
      <input type="text" name="value" placeholder="Value" required v-model="currentFilter.value">
      <small>The LIKE query syntax is supported. See <a target="_blank" href="https://sqlite.org/lang_expr.html#like">LIKE</a>.</small>
    </label>
  </form>
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
