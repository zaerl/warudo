<script setup lang="ts">

defineProps(['modelValue'])
defineEmits(['update:modelValue'])

</script>

<template>
<div id="search" class="container-fluid">
  <input
    type="search"
    id="search"
    name="search"
    placeholder="Search"
    :value="modelValue"
    @input="$emit('update:modelValue', ($event?.target as HTMLInputElement)?.value)" />
</div>
</template>
