"""Shared synthetic linked-result cases for receipt construction and reports."""
REQUEST = [{'address': '0x10', 'name': 'prior'}, {'address': '0x20', 'name': 'target'}]
TARGETS = [REQUEST[1]]


def results(statuses):
    return [dict(identity, status=status) for identity, status in zip(REQUEST, statuses)]


def linked_cases():
    # Receipt construction may retain unselected unknowns; reports stay partial.
    for name, before, after, receipt, report in (
            ('selected_gain', ['ok', 'mismatch'], ['ok', 'ok'], True, 'pass'),
            ('selected_still_mismatch', ['ok', 'mismatch'], ['ok', 'mismatch'], False, 'partial'),
            ('selected_not_checkable', ['ok', 'mismatch'], ['ok', 'not-checkable'], False, 'fail'),
            ('prior_ok_regression', ['ok', 'mismatch'], ['mismatch', 'ok'], False, 'fail'),
            ('selected_prior_ok_regression', ['ok', 'ok'], ['ok', 'mismatch'], False, 'fail'),
            ('selected_stays_ok', ['ok', 'ok'], ['ok', 'ok'], True, 'pass'),
            ('unselected_improvement', ['mismatch', 'mismatch'], ['ok', 'ok'], False, 'fail'),
            ('unselected_other_change', ['mismatch', 'mismatch'], ['missing', 'ok'], False, 'fail'),
            ('prior_mismatch_preserved', ['mismatch', 'mismatch'], ['mismatch', 'ok'], True, 'pass'),
            ('prior_not_checkable', ['not-checkable', 'mismatch'], ['not-checkable', 'ok'], True, 'partial'),
            ('prior_missing', ['missing', 'mismatch'], ['missing', 'ok'], True, 'partial'),
            ('prior_error', ['error', 'mismatch'], ['error', 'ok'], False, 'partial')):
        yield name, results(before), results(after), TARGETS, receipt, report
    before, after = results(['ok', 'mismatch']), results(['ok', 'ok'])
    extra = {'address': '0x30', 'name': 'unexpected', 'status': 'ok'}
    for name, old, new, targets in (
            ('duplicate_baseline', before + [before[1]], after, TARGETS),
            ('duplicate_candidate', before, after + [after[1]], TARGETS),
            ('missing_prior', before, after[1:], TARGETS),
            ('missing_selected', before, after[:1], TARGETS),
            ('unexpected_candidate', before, after + [extra], TARGETS),
            ('wrong_selected_name', before, [after[0], dict(after[1], name='wrong')], TARGETS),
            ('empty_scope', before, after, []),
            ('duplicate_scope', before, after, TARGETS * 2),
            ('unknown_scope', before, after, [{'address': '0x30', 'name': 'unexpected'}]),
            ('empty_target_name', before, after, [{'address': '0x20', 'name': ''}])):
        yield name, old, new, targets, False, 'invalid'
