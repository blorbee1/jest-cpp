import React from 'react';
import clsx from 'clsx';
import Link from "@docusaurus/Link";
import useDocusaurusContext from "@docusaurus/useDocusaurusContext";
import Layout from "@theme/Layout";
import CodeBlock from "@theme/CodeBlock";

import styles from "./index.module.css";

const sample = `#include <jest-cpp.hpp>
using namespace jest;

int main(int argc, char** argv) {
    init(argc, argv);
    
    describe("Counter") {
        Counter c; // locals in a describe body work
        
        it ("starts at zero") {
            expect(c.value()).toBe(0):
        };
        
        it.each({1, 2, 3})("add %i", [&](int n) {
            for (int i = 0; i < n; ++i) c.inc();
            expect(c.value()).toBeGreaterThan(0);
        });
    }
    
    return run();
}`;

const output = `Counter
    PASS starts at zero
    PASS adds 1
    PASS adds 2
    PASS adds 3
    
Tests:  0 failed, 4 passed, 4 total`;

const features = [
    {
        title: "Looks like Jest",
        text: 'describe, it, test, expect, hooks, and .each, written as blocks: describe("name") { .... }.'
    },
    {
        title: "Table driven tests",
        text: 'it.each, test.each, and describe.each with printf style titles, and test.failing for known bugs.'
    },
    {
        title: "Rich matchers",
        text: "Deep and shallow equality, std::optional and pointer support, regexes, exceptions, and .never to negate."
    },
    {
        title: "Readable failures",
        text: "Values are pretty printed: [1, 2, 3], Optional(5), 0.30000000000000004. Quiet by default, --verbose can be used"
    },
    {
        title: "One header, no dependencies",
        text: "Copy jest-cpp.hpp or use CMake FetchContent. Clean build under -Wall -Wextra -Wpedantic -Wshadow -Wconverstion"
    }
];

function Hero() {
    const { siteConfig } = useDocusaurusContext();
    return (
        <header className={clsx('hero hero--primary', styles.hero)}>
            <div className="container">
                <h1 className="hero__title">{siteConfig.title}</h1>
                <p className="hero__subtitle">{siteConfig.tagline}</p>
                <div className={styles.buttons}>
                    <Link className="button button--secondary button--lg" to="/docs/getting-started">Getting Started</Link>
                    <Link className={clsx("button button--outline button--lg", styles.ghost)} to="/docs/matchers">Matchers</Link>
                </div>
            </div>
        </header>
    )
}

function Showcase() {
    return (
        <section className={styles.showcase}>
            <div className="container">
                <div className="row">
                    <div className="col col--7">
                        <CodeBlock language="cpp" title="tests.cpp">{sample}</CodeBlock>
                    </div>
                    <div className="col col--5">
                        <CodeBlock language="text" title="$ ./tests --verbose">{output}</CodeBlock>
                        <p className={styles.note}>
                            Without <code>--verbose</code> only failures and the summary are printed
                        </p>
                    </div>
                </div>
            </div>
        </section>
    )
}

function Features() {
    return (
        <section className={styles.features}>
            <div className="container">
                <div className="row">
                    {features.map(({title, text}) => (
                        <div key={title} className={clsx('col col--4', styles.feature)}>
                            <h3>{title}</h3>
                            <p>{text}</p>
                        </div>
                    ))}
                </div>
            </div>
        </section>
    )
}

export default function Home() {
    const {siteConfig} = useDocusaurusContext();
    return (
        <Layout title={siteConfig.title} description={siteConfig.tagline}>
            <Hero/>
            <main>
                <Showcase/>
                <Features/>
            </main>
        </Layout>
    )
}