namespace java edu.umass.cs.iesl.wikilink.expanded.data
namespace py edu.umass.cs.iesl.wikilink.expanded.data

struct PageContentItem {
  // original download
  1: optional binary raw,

  // all visible text, e.g. from boilerpipe 1.2.0 KeepEverything
  2: optional string fullText, 

  // all visible text, e.g. from boilerpipe 1.2.0 Article text extractor
  3: optional string articleText, 

  // a correctly parsed and reformatted HTML version of raw with each
  // HTML tag on its own line
  4: optional string dom 
}

struct Context {

  1: string left,

  2: string right,

  3: string middle

}

struct Mention {

  1: string wiki_url,

  2: string anchor_text,

  3: i32 raw_text_offset,

  4: optional Context context,

  5: optional string freebase_id

}

struct RareWord {

  1: string word
  
  2: i32 offset
  
}

struct WikiLinkItem {
  // id from google release
  1: i32 doc_id, 
  
  // the original URL string obtain from some source
  2: string url, 
  
  // primary content
  3: PageContentItem content,   
  
  // rare words from google data
  4: list<RareWord> rare_words,

  // array of annotation objects for the document
  5: list<Mention> mentions
}
