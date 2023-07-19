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
  inline?: boolean,
  spaces?: number,
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
  rvalue: string | number | boolean | null,
  value: string | number | boolean | null,
  end: number | null,
  hasFollow: boolean,
  isParent?: boolean,
  isClosed?: boolean,
  hiddenBy: number | null,
};

function addBranch(tree: JSONBranch[], type: JSONType, indent: number, name: string | null = null,
  value: string | number | boolean | null = null, end: number | null = null ) {
  tree.push({type, indent, name, rvalue: null, value, end, hasFollow: false, isClosed: false, hiddenBy: null });
}

function getObject(object: { [x: string]: JSONValue } | JSONValue[], tree: JSONBranch[], indent = 0, space = 2): string {
  let isArray = Array.isArray(object);
  let output = isArray ? '[' : '{';
  let length = isArray ? (object as JSONValue[]).length : Object.keys(object).length;
  let i = 0;

  addBranch(tree, isArray ? JSONType.ArrayOpen : JSONType.ObjectOpen, indent, null, output);
  let start = tree.length - 1;

  tree[start].isParent = true;
  tree[start].rvalue = isArray ? ']' : '}';
  // tree[start].isClosed = true;

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
  jsonTree[0].isParent = false;
}

let codeTree = ref<JSONBranch[]>(jsonTree);

function clickBranch(index: number) {
  if(!codeTree.value[index].isParent) {
    return;
  }

  const parent = codeTree.value[index];
  const isClosed = parent.isClosed;

  if(index + 1 >= codeTree.value.length) {
    return;
  }

  parent.isClosed = !isClosed;

  if(isClosed) {
    // Open all children.
    for(let i = index + 1; i <= (parent.end as number); ++i) {
      if(codeTree.value[i].hiddenBy === index) {
        codeTree.value[i].hiddenBy = null;
      }
    }
  } else {
    // Close all children.
    for(let i = index + 1; i <= (parent.end as number); ++i) {
      if(codeTree.value[i].hiddenBy === null) {
        codeTree.value[i].hiddenBy = index;
      }
    }
  }
}

function itemStyle(item: JSONBranch) {
  let indent = item.indent;

  if(item.isParent) {
    indent -= 1;
  }

  indent *= props.spaces;

  return {
    'margin-left': indent + 'em',
  };
}

function itemClasses(item: JSONBranch) {
  const classes: { [name: string]: boolean | number } = {
    'is-child': !item.isParent,
    'is-parent': item.isParent as boolean,
    'is-closed': item.isClosed as boolean,
    'is-hidden': item.hiddenBy !== null,
  };

  classes[`level-${item.indent}`] = true;

  return classes;
}

function itemNameStyle(item: JSONBranch) {
  return {
    'padding-left': item.isParent ? '6px' : 0
  };
}
</script>

<template>
<section v-if="json" :class="{ inline: props.inline }">
  <pre v-if="props.inline"><code v-html="json"></code></pre>
  <code v-else class="tree">
    <div class="treeitem"
      v-for="(item, index) in codeTree" v-bind:key="index"
      :class="itemClasses(item)"
      :style="itemStyle(item)"
      @click="clickBranch(index)">
      <span class="name" v-if="item.name !== null" :style="itemNameStyle(item)">"<b>{{ item.name }}</b>: </span>
      <template v-if="item.value !== null">
        <span class="value" v-if="item.type === JSONType.String"><u>"{{ item.value }}"</u></span>
        <span class="value" v-else-if="item.type === JSONType.Number"><i>{{ item.value }}</i></span>
        <span class="value" v-else-if="item.type === JSONType.Boolean"><strong><i>{{ item.value }}</i></strong></span>
        <span class="value" v-else>{{ item.value }}</span>
        <span v-if="item.isParent" class="hellip"><em>&nbsp;&hellip;&nbsp;</em>{{ item.rvalue }}</span>
      </template>
      <span class="follow" v-if="item.hasFollow">,</span>
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

/*.is-parent.is-closed > .tree-expand:before {
  content: "▾";
}*/

.name, .value {
  display: inline-block;
}

.treeitem .value {
  padding-left: 7px;
}

.treeitem.level-0 .value {
  padding-left: 0;
}

.treeitem {
  display: block;
}

.treeitem.is-hidden {
  display: none;
}

.is-parent {
  cursor: pointer;
}
.is-parent::before {
  background-image: var(--icon-chevron);
  content: "";
  width: 21px;
  height: 21px;
  display: block;
  float: left;
  color: var(--code-color);
  /*-webkit-margin-start: calc(var(--spacing, 1rem) * 0.5);
  margin-inline-start: calc(var(--spacing, 1rem) * 0.5);
  margin-left: 3px;*/
  background-position: right center;
  background-size: 21px auto;
  background-repeat: no-repeat;
  transition: 0.3s ease-in-out,-webkit-transform .3s ease-in-out;
}

.is-parent.is-closed::before {
  transform: rotate(-90deg);
}

.is-parent::before {
  -webkit-transform: rotate(0);
  transform: rotate(0);
}

.is-child.is-closed {
  display: none;
}

.hellip {
  display: none;
}

.is-closed .hellip {
  display: inline-block;
}

</style>
