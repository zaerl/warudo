<script setup lang="ts">

const emit = defineEmits(['close']);

interface Props {
  isOpen?: boolean,
  title?: string,
  description?: string,
  cancelButton?: string,
  confirmButton?: string,
}

const props = withDefaults(
  defineProps<Props>(), {
    isOpen: false,
    title: '',
    description: '',
    cancelButton: 'Cancel',
    confirmButton: 'Confirm',
  }
);

function close() {
  emit('close', false);
}

function confirm() {
  emit('close', true);
}

</script>

<template>
<dialog :open="props.isOpen">
  <article>
    <header>
        <a aria-label="Close" class="close" @click="close"></a>
        {{ props.title }}
    </header>
    <slot>{{ props.description }}</slot>
    <footer>
      <a
        href=""
        role="button"
        class="secondary"
        @click.prevent="close">
        {{ props.cancelButton }}
      </a>
      <a
        href=""
        role="button"
        @click.prevent="confirm">
        {{ props.confirmButton }}
      </a>
    </footer>
  </article>
</dialog>
</template>
