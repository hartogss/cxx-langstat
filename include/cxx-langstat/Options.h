#ifndef OPTIONS_H
#define OPTIONS_H


enum Stage {
    none, emit_features=1, emit_statistics=2
};
enum AnalysisType {
    cca, lda, lka, msa, sla, sla2, tia, tpa, ua, vta
};
enum InputType {
    file, dir
};


#endif // OPTIONS_H
