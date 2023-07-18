import type { OrderBy, SearchBarParams, Sort } from "./api";

export function getQuery(search: string, query: SearchBarParams) {
  const ret: SearchBarParams = {};

  if(typeof search == 'string' && search.length > 0) {
    ret.key = 'any';
    ret.value = search;
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
