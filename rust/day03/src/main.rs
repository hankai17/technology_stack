use regex_automata::nfa::thompson;
use regex_automata::dfa::{dense, Automaton};

fn test1() -> Result<(), Box<dyn std::error::Error>> {
    let pattern = r"a(b|c)*d";

    // 1. 未最小化 DFA
    let dfa_unmin = dense::Builder::new()
        .configure(dense::Config::new().minimize(false))
        .build(pattern)?;

    // 2. 最小化 DFA
    let dfa_min = dense::Builder::new()
        .configure(dense::Config::new().minimize(true))
        .build(pattern)?;

    // state_len() 来自 Automaton trait，现在可用了
    //println!("--- 未最小化 DFA 状态数: {} ---", dfa_unmin.state_len());
    //println!("--- 最小化后 DFA 状态数: {} ---", dfa_min.state_len());
    println!("{:#?}", dfa_unmin);
    println!("{:#?}", dfa_min);

    // 打印最小化 DFA 的调试信息
    println!("\n=== Minimized DFA Debug ===");
    println!("{:#?}", dfa_min);

    Ok(())
}


fn main() -> Result<(), Box<dyn std::error::Error>> {
    let pattern = r"a(b|c)*d";

    println!("==============================");
    println!("Regex: {}", pattern);
    println!("==============================");

    // -----------------------------
    // 1. Thompson NFA
    // -----------------------------
    let nfa = thompson::Compiler::new().build(pattern)?;

    println!("\n========== Thompson NFA ==========");
    println!("{:#?}", nfa);

    // -----------------------------
    // 2. DFA（未最小化）
    // -----------------------------
    let dfa = dense::Builder::new()
        .configure(
            dense::Config::new()
                .minimize(false),
        )
        .build(pattern)?;

    println!("\n========== DFA (Before Minimize) ==========");
    println!("{:#?}", dfa);

    // -----------------------------
    // 3. DFA（最小化）
    // -----------------------------
    let min_dfa = dense::Builder::new()
        .configure(
            dense::Config::new()
                .minimize(true),
        )
        .build(pattern)?;

    println!("\n========== DFA (After Minimize) ==========");
    println!("{:#?}", min_dfa);

    Ok(())
}
