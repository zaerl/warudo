<script setup lang="ts">
import type { GetDataType } from '@/data/api';
import { getData, type Key } from '@/data/api';
import { onMounted, ref } from 'vue';

interface Props {
  type: GetDataType,
  showCounts?: boolean,
  modelValue?: string,
}

const props = withDefaults(
  defineProps<Props>(), {
    type: 'keys',
    showCounts: false,
    modelValue: '',
  }
);

defineEmits(['update:modelValue']);

let busy = ref(true);
let invalid = ref(false);
let keys = ref<Key[] | null>([]);

function keyName(key: Key) {
  return props.showCounts ? `${key.name} ${key.value}` : key.name;
}

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
<select
  v-else
  required
  :aria-busy="busy"
  id="keys-list"
  :value="props.modelValue"
  @input="$emit('update:modelValue', ($event?.target as HTMLSelectElement)?.value)">
  <option disabled value="">Please select one key</option>
  <option v-bind:key="key.name" v-for="key in keys" :value="key.name">{{ keyName(key) }}</option>
</select>
</template>

<style scoped></style>
