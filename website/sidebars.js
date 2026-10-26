// @ts-check

/** @type {import("@docusaurus/plugin-content-docs").SidebarsConfig} */
const sidebars = {
    docs: [
        {
            type: "category",
            label: "Guide",
            collapsible: false,
            items: ["getting-started", "writing-tests", "each", "failing-and-timeouts", "command-line"]
        },
        {
            type: "category",
            label: "Reference",
            collapsible: false,
            items: ["matchers", "differences-from-jest"]
        },
        {
            type: "category",
            label: "Examples",
            collapsible: false,
            items: ["examples"]
        }
    ]
};

module.exports = sidebars;