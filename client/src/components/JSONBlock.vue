<script setup lang="ts">

export type JSONValue =
  | string
  | number
  | boolean
  | { [x: string]: JSONValue }
  | Array<JSONValue>;

interface Props {
  json: { [x: string]: JSONValue } | null,
  inline: boolean,
}

const props = withDefaults(
  defineProps<Props>(), {
    inline: false,
    json: null,
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
  type: JSONValue,
  indent: number,
  name: string | null,
  value: string | number | boolean | null,
  end: number | null,
  hasFollow: boolean,
  open?: boolean,
};

function addBranch(tree: JSONBranch[], type: JSONValue, indent: number, name: string | null = null,
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

  tree[start].open = true;

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
        addBranch(tree, type, indent + 1, treeName, value as string);
        break;
      case 'number':
        rvalue = `<i>${value}</i>`;
        addBranch(tree, type, indent + 1, treeName, value as number);
        break;
      case 'boolean':
        rvalue = `<strong><i>${value}</i></strong>`;
        addBranch(tree, type, indent + 1, treeName, value as boolean);
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
        addBranch(tree, 'string', indent + 1, treeName, value as string);
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
</script>

<template>
<section v-if="json" :class="{ inline: props.inline }">
  <code class="tree">
    <div class="treeitem" v-for="(line, index) in jsonTree" v-bind:key="index" :class="{ 'is-open': line.open }" :style="{ 'margin-left': (line.indent * 2) + 'ch' }">
      <span class="name" v-if="line.name !== null">"<b>{{ line.name }}"</b>: </span>
      <template v-if="line.value !== null">
        <span class="value" v-if="line.type === JSONType.String"><u>"{{ line.value }}"</u></span>
        <span class="value" v-else-if="line.type === JSONType.Number"><i>{{ line.value }}</i></span>
        <span class="value" v-else-if="line.type === JSONType.Boolean"><strong><i>{{ line.value }}</i></strong></span>
        <span class="value" v-else>{{ line.value }}</span>
      </template>
      <span class="follow" v-if="line.hasFollow">,</span>
    </div>
  </code>
  <!--<pre><code v-html="json"></code></pre>-->
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

.is-open {
  font-weight: bold;
}
</style>
