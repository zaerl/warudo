<script setup lang="ts">

export type JSONValue =
  | string
  | number
  | boolean
  | { [x: string]: JSONValue }
  | Array<JSONValue>;

interface Props {
  json: { [x: string]: JSONValue },
}

const props = defineProps<Props>();

function getIndent(indent: number, space = 2): string {
  let result = '';

  for(let i = 0; i < indent * space; i++) {
    result += '&nbsp;';
  }

  return result;
}

function getObject(object: { [x: string]: JSONValue }, indent = 0, space = 2): string {
  let output = '{';
  let length = Object.keys(object).length;
  let i = 0;

  for(const property in object) {
    const value = object[property];
    let spaces = getIndent(indent + 1, space);
    let type: string = typeof value;

    output += "\n" + spaces + `<b>"${property}"</b>: `;

    if(type === 'object' && Array.isArray(value)) {
      type = 'array';
    }

    switch(type) {
      case 'string':
        output += `<u>"${value}"</u>`;
        break;
      case 'number':
        output += `<i>${value}</i>`;
        break;
      case 'boolean':
        output += `<strong><i>${value}</i></strong>`;
        break;
      case 'array':
        output += '[]';
        break;
      case 'object':
        output += getObject(value as { [x: string]: JSONValue }, indent + 1, space);
        break;
      default:
        output += `"${value}"`;
        break;
    }

    if(i === length - 1) {
      output += "\n" + getIndent(indent, space) + '}';
    } else {
      output += ',';
    }

    ++i;
  }

  return output;
}

const json = getObject(props.json, 0);
</script>

<template>
<section>
  <pre><code v-html="json"></code></pre>
</section>
</template>

<style scoped>
section > pre {
  background: var(--article-code-background-color);
  box-shadow: var(--card-box-shadow);
}
</style>
