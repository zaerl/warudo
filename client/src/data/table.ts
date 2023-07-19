import type { OrderBy, SearchBarParams, Sort } from "./api";

export function getQuery(search: string, query: SearchBarParams) {
  const ret: SearchBarParams = {};

  if(typeof search == 'string' && search.length > 0) {
    const split = search.split(/\s+/);

    if(split.length >= 2 && split[0].length > 0 && split[1].length > 0) {
      ret.key = split[0].startsWith('$.') ? split[0] : '$.' + split[0];
      split.shift();

      ret.value = split.join(' ');
    }
  }

  if(typeof query === 'undefined') {
    return ret;
  }

  if(typeof query.sort !== 'undefined' && query.sort !== null) {
    ret.sort = query.sort as Sort;
  }

  if(typeof query.orderby !== 'undefined' && query.orderby !== null) {
    ret.orderby = query.orderby as OrderBy;
  }

  return ret;
}

export function defaultHeaders(key = 'data', name = 'Data') {
  return [
    {
      key: 'id',
      name: '#',
    },
    {
      key: 'created',
      name: 'Created',
    },
    {
      key,
      name,
    },
  ];
}
