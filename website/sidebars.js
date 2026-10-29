// @ts-check

/** @type {import("@docusaurus/plugin-content-docs").SidebarsConfig} */
const sidebars = {
    docs: [
        {
            type: "category",
            label: "Guide",
            collapsible: false,
            items: ["getting-started", "writing-tests", "each", "failing-and-timeouts", "mock-functions", "command-line", "compile-times", "migrating"]
        },
        {
            type: "category",
            label: "Reference",
            collapsible: false,
            items: ["matchers", "differences-from-jest"]
        },
        {
            type: "doc",
            label: "Examples",
            id: "examples"
        }
    ]
};

module.exports = sidebars;