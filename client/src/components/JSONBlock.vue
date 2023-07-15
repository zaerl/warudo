<script setup lang="ts">
import { ref } from 'vue';

export type JSONValue =
  | string
  | number
  | boolean
  | { [x: string]: JSONValue }
  | Array<JSONValue>;

interface Props {
  json: { [x: string]: JSONValue } | null,
  inline: boolean,
  spaces: number,
}

const props = withDefaults(
  defineProps<Props>(), {
    inline: false,
    json: null,
    spaces: 2,
  }
);

function getIndent(indent: number, space = 2): string {
  if(props.inline) {
    return '';
  }

  let result = "\n";

  for(let i = 0; i < indent * space; i++) {
    result += '&nbsp;';
  }

  return result;
}

enum JSONType {
  String = 'string',
  Number = 'number',
  Boolean = 'boolean',
  ObjectOpen = '{',
  ObjectClose = '}',
  ArrayOpen = '[',
  ArrayClose = ']',
}

interface JSONBranch {
  type: JSONType,
  indent: number,
  name: string | null,
  value: string | number | boolean | null,
  end: number | null,
  hasFollow: boolean,
  canOpen?: boolean,
  isOpen?: boolean,
};

function addBranch(tree: JSONBranch[], type: JSONType, indent: number, name: string | null = null,
  value: string | number | boolean | null = null, end: number | null = null ) {
  tree.push({type, indent, name, value, end, hasFollow: false });
}

function getObject(object: { [x: string]: JSONValue } | JSONValue[], tree: JSONBranch[], indent = 0, space = 2): string {
  let isArray = Array.isArray(object);
  let output = isArray ? '[' : '{';
  let length = isArray ? (object as JSONValue[]).length : Object.keys(object).length;
  let i = 0;

  addBranch(tree, isArray ? JSONType.ArrayOpen : JSONType.ObjectOpen, indent, null, output);
  let start = tree.length - 1;

  tree[start].canOpen = true;
  tree[start].isOpen = true;

  if(props.inline) {
    output += ' ';
  }

  for(const property in object) {
    const value = isArray ? (object as JSONValue[])[property as unknown as number] :
        (object as { [x: string]: JSONValue })[property];
    let spaces = getIndent(indent + 1, space);
    let type: string = typeof value;

    if(isArray) {
      output += spaces;
    } else {
      output += spaces + `<b>"${property}"</b>: `;
    }

    if(type === 'object' && Array.isArray(value)) {
      type = 'array';
    }

    let rvalue = '';
    let index = tree.length - 1;
    let treeName = isArray ? null : property;

    switch(type) {
      case 'string':
        rvalue = `<u>"${value}"</u>`;
        addBranch(tree, JSONType.String, indent + 1, treeName, value as string);
        break;
      case 'number':
        rvalue = `<i>${value}</i>`;
        addBranch(tree, JSONType.Number, indent + 1, treeName, value as number);
        break;
      case 'boolean':
        rvalue = `<strong><i>${value}</i></strong>`;
        addBranch(tree, JSONType.Boolean, indent + 1, treeName, value as boolean);
        break;
      case 'array':
        rvalue = getObject(value as JSONValue[], tree, indent + 1, space);
        tree[index + 1].name = treeName;
        break;
      case 'object':
        rvalue = getObject(value as { [x: string]: JSONValue }, tree, indent + 1, space);
        tree[index + 1].name = treeName;
        break;
      default:
        rvalue = `"${value}"`;
        addBranch(tree, JSONType.String, indent + 1, treeName, value as string);
        break;
    }

    if(i !== length - 1) {
      rvalue += props.inline ? ', ' : ',';
      tree[tree.length - 1].hasFollow = true;
    }

    output += rvalue;

    ++i;
  }

  if(props.inline) {
    output += ' ';
  }

  output += getIndent(indent, space) + (isArray ? ']' : '}');

  addBranch(tree, isArray ? JSONType.ArrayClose : JSONType.ObjectClose, indent, null, isArray ? ']' : '}');
  tree[start].end = tree.length - 1;

  return output;
}

const jsonTree: JSONBranch[] = [];
const json: string | null = props.json ? getObject(props.json, jsonTree) : null;

if(jsonTree.length > 0) {
  jsonTree[0].canOpen = false;
}

let codeTree = ref<JSONBranch[]>(jsonTree);

function clickBranch(index: number) {
  codeTree.value[index].isOpen = !codeTree.value[index].isOpen;
}

function itemStyle(item: JSONBranch) {
  let indent = item.indent * props.spaces;

  if(item.canOpen) {
    --indent;
  }

  indent *= 14;

  return {
    'margin-left': indent + 'px',
  };
}

function itemClasses(item: JSONBranch) {
  return {
    'can-open': item.canOpen,
    'is-open': item.isOpen,
  };
}

function itemNameStyle(item: JSONBranch) {
  return {
    'padding-left': item.canOpen ? '5px' : 0
  };
}

//
</script>

<template>
<section v-if="json" :class="{ inline: props.inline }">
  <pre v-if="props.inline"><code v-html="json"></code></pre>
  <code class="tree" v-else>
    <div class="treeitem"
      v-for="(line, index) in codeTree" v-bind:key="index"
      :class="itemClasses(line)"
      :style="itemStyle(line)"
      @click="clickBranch(index)">
      <span class="tree-expand"></span>
      <span class="name" v-if="line.name !== null" :style="itemNameStyle(line)">"<b>{{ line.name }}"</b>: </span>
      <template v-if="line.value !== null">
        <span class="value" v-if="line.type === JSONType.String"><u>"{{ line.value }}"</u></span>
        <span class="value" v-else-if="line.type === JSONType.Number"><i>{{ line.value }}</i></span>
        <span class="value" v-else-if="line.type === JSONType.Boolean"><strong><i>{{ line.value }}</i></strong></span>
        <span class="value" v-else>{{ line.value }}</span>
      </template>
      <span class="follow" v-if="line.hasFollow">,</span>
    </div>
  </code>
</section>
</template>

<style scoped>
section > pre {
  background: var(--article-code-background-color);
  box-shadow: var(--card-box-shadow);
}

section.inline {
  margin: 0;
}

section > pre {
  white-space: break-spaces;
}

section.inline > pre {
  display: inline-block;
  margin: 0;
  padding: 0;
  box-shadow: none;
}

section.inline > pre > code {
  margin: 0;
  padding: 0;
  white-space: normal;
}

.tree {
  display: block;
  background: var(--article-code-background-color);
  box-shadow: var(--card-box-shadow);
  padding: var(--spacing);
  margin-bottom: var(--spacing);
  font-size: 14px;
  line-height: var(--line-height);
  /*background: var(--article-code-background-color);
  box-shadow: var(--card-box-shadow);
  font-family: "Menlo", "Consolas", "Roboto Mono", "Ubuntu Monospace",
    "Noto Mono", "Oxygen Mono", "Liberation Mono", monospace,
    "Apple Color Emoji", "Segoe UI Emoji", "Segoe UI Symbol", "Noto Color Emoji";
  font-size: 14px;
  line-height: var(--line-height);
  color: var(--code-color);
  */
}

.tree .value {
  padding-left: 7px;
}

.can-open {
  cursor: pointer;
}

/*.can-open.is-open > .tree-expand:before {
  content: "▾";
}*/

.name, .value {
  display: inline-block;
}

.tree-expand {
  display: none;
  transform: rotate(0);
  transition: 0.3s ease-in-out,-webkit-transform .3s ease-in-out;
}

.can-open .tree-expand {
  display: inline-block;
}

.is-open .tree-expand {
  transform: rotate(90deg);
}

.tree-expand:before {
  content: "▸";
}

</style>
