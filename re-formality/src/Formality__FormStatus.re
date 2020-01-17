type t('submissionError) =
  | Editing
  | Submitting(option('submissionError))
  | Submitted
  | SubmissionFailed('submissionError);

type submission =
  | NeverSubmitted
  | AttemptedToSubmit;
